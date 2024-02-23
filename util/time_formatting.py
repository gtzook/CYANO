from typing import Tuple
from datetime import datetime as dt
import datetime

def sec_to_time(seconds: int) -> Tuple[int,int,int]:
    hours = seconds // (60 * 60)
    seconds -= 60*60*hours
    minutes = seconds // 60
    secs = seconds - (minutes * 60)
    return int(hours), int(minutes), int(secs)

def to_two_digit_string(value: int) -> str:
    ret = str(int(value))
    if value > 99:
        ret = "99"
    if value < 10:
        ret = "0" + ret
    return ret

def time_string_from_hr_min_sec(hours: int, minutes: int, sec: int) -> str:
    return (to_two_digit_string(hours) + ":" +
            to_two_digit_string(minutes) + ":" +
            to_two_digit_string(sec))
    
def time_string_from_sec(seconds: int) -> str:
    hrs,mins,secs = sec_to_time(seconds)
    return time_string_from_hr_min_sec(hrs,mins,secs)

def isTimeFormat(input: str)-> dt.time:
    try:
        x=dt.strptime(input, '%H:%M:%S')
        return x.time()
    except ValueError:
        print("Invalid time string. Should be HH:MM:SS")
        return  None

def getTimeFromUser() -> dt.time:
    user_str = ""
    ret = None
    while ret == None:
        user_str = input()
        ret = isTimeFormat(user_str)
    return ret

def is_time_between(begin_time: dt.time, end_time: dt.time, check_time=None):
    # If check time is not given, default to current time
    check_time = check_time or dt.now().time()
    if begin_time < end_time:
        return check_time >= begin_time and check_time <= end_time
    else: # crosses midnight
        return check_time >= begin_time or check_time <= end_time
    
def isDay(night, day):
    return is_time_between(day,night)

def seconds_until(time: dt.time):
    dttime =  dt.combine(datetime.date.today(), time)
    left = (dttime - dt.now()).total_seconds()
    if left > 0:
        return left
    # if time is negative, this time has already passed, so check tommorow
    dttime =  dt.combine(datetime.date.today() + datetime.timedelta(days=1), time)
    return (dttime - dt.now()).total_seconds()