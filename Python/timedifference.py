import time
import datetime

s1 = datetime.datetime.now()

time.sleep(10)

s2 = datetime.datetime.now()

tdelta = s2 - s1

print(tdelta.hour)