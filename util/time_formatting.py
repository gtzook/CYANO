def sec_to_time(seconds):
    hours = seconds // (60 * 60)
    seconds -= 60*60*hours
    minutes = seconds // 60
    secs = seconds - (minutes * 60)
    return hours, minutes, secs

def to_two_digit_string(value):
    ret = str(int(value))
    if value > 99:
        ret = "99"
    if value < 10:
        ret = "0" + ret
    return ret

def time_string_from_hr_min_sec(hours, minutes, sec):
    return (to_two_digit_string(hours) + ":" +
            to_two_digit_string(minutes) + ":" +
            to_two_digit_string(sec))
    
def time_string_from_sec(seconds):
    hrs,mins,secs = sec_to_time(seconds)
    return time_string_from_hr_min_sec(hrs,mins,secs)