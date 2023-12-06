from typing import Tuple
import time

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

def isTimeFormat(input: str)-> bool:
    try:
        x=time.strptime(input, '%H:%M:%S')
        return x
    except ValueError:
        print("Invalid time string. Should be HH:MM:SS")
        return  None

def getTimeFromUser() -> time.struct_time:
    user_str = ""
    ret = None
    while ret == None:
        user_str = input()
        ret = isTimeFormat(user_str)
    return ret
