import time
from machine import WDT
def on_wdt(self):
    print(self.context(), self)
    self.feed()
    ## release WDT
    #self.stop()

# test callback wdt
wdt1 = WDT(id=1, timeout=4000, callback=on_wdt, context={})
print('into', wdt1)
time.sleep(2)
print(time.ticks_ms())
# 1.test wdt feed
wdt1.feed()
time.sleep(2)
print(time.ticks_ms())
# 2.test wdt stop
# wdt1.stop()
# print('stop', wdt1)
# 3.wait wdt work
while True:
    print('idle', time.ticks_ms())
    time.sleep(1)
