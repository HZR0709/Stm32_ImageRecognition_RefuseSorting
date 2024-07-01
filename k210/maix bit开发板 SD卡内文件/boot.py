import KPU as kpu
import sensor
import lcd
from Maix import GPIO
from fpioa_manager import fm
from board import board_info
import time
import gc
import image
from machine import UART
from machine import WDT
import sys

class ImagePool:
    def __init__(self, count, width, height, format=sensor.RGB565):
        self.pool = [image.Image(width, height, format=format) for _ in range(count)]
        self.available = count

    def get_image(self):
        if self.available > 0:
            self.available -= 1
            return self.pool[self.available]
        else:
            return None

    def release_image(self, img):
        if self.available < len(self.pool):
            self.pool[self.available] = img
            self.available += 1


class ClassifierControl:
    def __init__(self):
        # 初始化分类器控制类
        self.class_num = 5
        self.sample_num = 5
        self.THRESHOLD = 11
        self.class_names = ['无', '可回收', '有害', '厨余', '其他']
        self.board_cube = 0
        self.saved_path = "5_classes.classifier"
        self.train_status = 0
        self.train_flag = 1
        self.img = None
        self.cap_num = 0
        self.last_cap_time = 0
        self.last_btn_status = 0
        self.uart = self.init_uart()
        self.init_sensor_lcd()
        self.key_confirm, self.key_up, self.key_down = self.init_buttons()
        self.img = image.Image()
        image.font_load(image.UTF8, 16, 16, '/sd/songti.Dzk')
        self.init_watchdog()

    def init_watchdog(self):
        # 初始化看门狗定时器，设置超时为 4000 毫秒
        self.wdt = WDT(id=1, timeout=1000*10, callback=self.on_wdt, context={})

    def feed_watchdog(self):
        # 喂食看门狗，重置计时器
        self.wdt.feed()

    def on_wdt(self, event):
        # 看门狗超时时的回调函数
        print("Watchdog triggered!")
        self.feed_watchdog()  # 重新喂食

    def init_uart(self):
        # 初始化 UART
        fm.register(6, fm.fpioa.UART1_RX, force=True)
        fm.register(7, fm.fpioa.UART1_TX, force=True)
        return UART(UART.UART1, 9600, read_buf_len=4096)

    def init_sensor_lcd(self):
        # 初始化传感器和 LCD
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QVGA)
        sensor.set_windowing((224, 224))
        if self.board_cube == 1:
            sensor.set_vflip(True)
            sensor.set_hmirror(True)
            lcd.init(type=2)
            lcd.rotation(2)
        else:
            lcd.init()

    def init_buttons(self):
        # 初始化按钮
        fm.register(10, fm.fpioa.GPIOHS0)
        key_confirm = GPIO(GPIO.GPIOHS0, GPIO.PULL_UP)
        fm.register(15, fm.fpioa.GPIOHS1)
        key_up = GPIO(GPIO.GPIOHS1, GPIO.PULL_UP)
        fm.register(16, fm.fpioa.GPIOHS2)
        key_down = GPIO(GPIO.GPIOHS2, GPIO.PULL_UP)
        return key_confirm, key_up, key_down

    def draw_string(self, x, y, text, color, scale, bg=None):
        # 在 LCD 上绘制字符串
        if self.img is None:
            print("Error: Image is None")
            return None
        self.img.draw_rectangle(x - 2, y - 2, len(text) * 8 * scale + 4, 16 * scale, fill=True, color=bg)
        self.img = self.img.draw_string(x, y, text, color=color, scale=scale, x_spacing=2, mono_space=1)

    def update_display(self, btn_status):
        # 根据当前状态更新显示
        self.draw_string(80, 100, b"训练新的分类器", color=lcd.WHITE, scale=1, bg=lcd.BLACK)
        self.draw_string(80, 120, b"使用已有分类器", color=lcd.WHITE, scale=1, bg=lcd.BLACK)
        if btn_status == 1:
            self.draw_string(70, 100, ' ', color=lcd.WHITE, scale=1, bg=lcd.BLACK)
            self.draw_string(70, 120, '>', color=lcd.WHITE, scale=1, bg=lcd.BLACK)
        else:
            self.draw_string(70, 100, '>', color=lcd.WHITE, scale=1, bg=lcd.BLACK)
            self.draw_string(70, 120, ' ', color=lcd.WHITE, scale=1, bg=lcd.BLACK)
        lcd.display(self.img)

    def user_selection(self):
        # 用户选择分类器类型
        btn_status = 0
        while True:
            if self.key_up.value() == 0:
                btn_status = 0
            if self.key_down.value() == 0:
                btn_status = 1
            self.update_display(btn_status)
            if self.key_confirm.value() == 0:  # 确定按键按下
                if btn_status == 1:  # 使用已有的分类器
                    self.train_status = 1
                    self.train_flag = 1
                elif btn_status == 0:  # 使用新的分类器
                    self.train_flag = 0
                    self.train_status = 0
                break
        return btn_status

    def handle_buttons(self):
        # 处理按键选择
        if self.train_flag == 1:
            model = kpu.load(0x300000)
            classifier, self.class_num, self.sample_num = kpu.classifier.load(model, self.saved_path)
        elif self.train_flag == 0:
            model = kpu.load(0x300000)
            classifier = kpu.classifier(model, self.class_num, self.sample_num)
        return classifier

    def handle_training(self, classifier):
        # 处理训练逻辑
        if self.key_confirm.value() == 0:
            time.sleep_ms(30)
            if self.key_confirm.value() == 0 and (self.last_btn_status == 1) and (time.ticks_ms() - self.last_cap_time > 500):
                self.last_btn_status = 0
                self.last_cap_time = time.ticks_ms()
                if self.cap_num < self.class_num:
                    index = classifier.add_class_img(self.img)
                    self.cap_num += 1
                    print("Added class image:", index)
                elif self.cap_num < self.class_num + self.sample_num:
                    index = classifier.add_sample_img(self.img)
                    self.cap_num += 1
                    print("Added sample image:", index)
            else:
                self.draw_string(2, 200, "Release confirm button, please", color=lcd.RED, scale=1, bg=lcd.BLACK)
        else:
            time.sleep_ms(30)
            if self.key_confirm.value() == 1:
                self.last_btn_status = 1
            if self.cap_num < self.class_num:
                self.draw_string(2, 200, b"按确定键添加分类"+self.class_names[self.cap_num], color=lcd.RED, scale=1, bg=lcd.BLACK)
            elif self.cap_num < self.class_num + self.sample_num:
                self.draw_string(0, 200, b"按确定键添加样本{}".format(self.cap_num-self.class_num), color=lcd.RED, scale=1, bg=lcd.BLACK)

    def start_training(self, classifier):
        # 开始训练分类器
        print("Starting training...")
        self.draw_string(30, 100, "Training...", color=lcd.RED, scale=1, bg=lcd.BLACK)
        lcd.display(self.img)
        classifier.train()
        print("Training completed")
        self.train_status = 1
        classifier.save(self.saved_path)
        return classifier

    def handle_prediction(self, classifier):
        # 处理预测逻辑
        res_index, min_dist = classifier.predict(self.img)
        print("{:.2f}".format(min_dist))
        if res_index >= 0 and min_dist < self.THRESHOLD:
            print("Predict result:", self.class_names[res_index])
            self.uart.write("result:{}".format(res_index))
            self.draw_string(2, 2, b"{}".format(self.class_names[res_index]), color=lcd.RED, scale=1, bg=lcd.BLACK)
        else:
            print("Unknown, maybe:", self.class_names[res_index])
            self.uart.write("result:{}".format(res_index))
            self.draw_string(2, 2, b"可能是 {}".format(self.class_names[res_index]), color=lcd.RED, scale=1, bg=lcd.BLACK)

    def capture_and_manage_images(self, classifier):
        # 图像捕获和管理
        while True:
            try:
                self.img = sensor.snapshot()
                self.feed_watchdog()  # 喂食看门狗
                # 进行图像预处理等操作
            except Exception as e:
                print("图像处理异常:", e)
            else:
                if self.board_cube:
                    self.img = self.img.rotation_corr(z_rotation=90)
                    self.img.pix_to_ai()
                if self.train_status == 0:
                    self.handle_training(classifier)
                if self.train_status == 0 and self.cap_num >= self.class_num + self.sample_num:
                    classifier = self.start_training(classifier)
                if self.train_status == 1:
                    self.handle_prediction(classifier)
                lcd.display(self.img)

# Main function
def main():
    control = ClassifierControl()  # 创建 ClassifierControl 类的实例
    while True:
        control.user_selection()  # 用户选择分类器类型
        classifier = control.handle_buttons()  # 处理按键选择
        control.capture_and_manage_images(classifier)  # 图像捕获和处理主循环
        control.feed_watchdog()  # 确保在主循环中定期喂食看门狗
        gc.collect()  # 清理垃圾，为程序运行提供更多可用内存

if __name__ == '__main__':
    main()
