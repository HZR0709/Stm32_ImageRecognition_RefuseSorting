
try:
    import gc, lcd, image, sys, os
    from Maix import GPIO
    from fpioa_manager import fm
    test_pin=16
    fm.fpioa.set_function(test_pin,fm.fpioa.GPIO7)
    test_gpio=GPIO(GPIO.GPIO7,GPIO.IN,GPIO.PULL_UP)
    lcd.init()
    lcd.clear(color=(255,0,0))
    lcd.draw_string(lcd.width()//2-68,lcd.height()//2-4, "Welcome to ", lcd.WHITE, lcd.RED)
    if test_gpio.value() == 0:
        print('PIN 16 pulled down, enter test mode')
        lcd.clear(lcd.PINK)
        lcd.draw_string(lcd.width()//2-68,lcd.height()//2-4, "Test Mode, wait ...", lcd.WHITE, lcd.PINK)
        import sensor
        import image
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QVGA)
        sensor.run(1)
        lcd.freq(16000000)
        while True:
            img=sensor.snapshot()
            lcd.display(img)

    loading = image.Image(size=(lcd.width(), lcd.height()))
    loading.draw_rectangle((0, 0, lcd.width(), lcd.height()), fill=True, color=(255, 0, 0))
    info = "Welcome to MaixPy"
    loading.draw_string(int(lcd.width()//2 - len(info) * 5), (lcd.height())//4, info, color=(255, 255, 255), scale=2, mono_space=0)
    v = sys.implementation.version
    vers = 'V{}.{}.{} : maixpy.sipeed.com'.format(v[0],v[1],v[2])
    loading.draw_string(int(lcd.width()//2 - len(info) * 6), (lcd.height())//3 + 20, vers, color=(255, 255, 255), scale=1, mono_space=1)
    lcd.display(loading)
    tf = None
    try:
            os.listdir("/sd/.")
    except Exception as e:
        tf ="SDcard not mount,use flash!"
        loading.draw_string(int(lcd.width()//2 - len(info) * 7), (lcd.height())//2 + 10, tf, color=(255, 255, 255), scale=1, mono_space=1)
    if not tf:
        tf ="SDcard is mount,use SD!"
        loading.draw_string(int(lcd.width()//2 - len(info) * 6), (lcd.height())//2 + 10, tf, color=(255, 255, 255), scale=1, mono_space=1)
    lcd.display(loading)
    del loading, v, info, vers
    gc.collect()
finally:
    gc.collect()
