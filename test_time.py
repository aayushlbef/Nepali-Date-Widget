
import ctypes
kernel32 = ctypes.WinDLL('kernel32')

class SYSTEMTIME(ctypes.Structure):
    _fields_ = [
        ('wYear', ctypes.c_uint16),
        ('wMonth', ctypes.c_uint16),
        ('wDayOfWeek', ctypes.c_uint16),
        ('wDay', ctypes.c_uint16),
        ('wHour', ctypes.c_uint16),
        ('wMinute', ctypes.c_uint16),
        ('wSecond', ctypes.c_uint16),
        ('wMilliseconds', ctypes.c_uint16)
    ]

class FILETIME(ctypes.Structure):
    _fields_ = [
        ('dwLowDateTime', ctypes.c_uint32),
        ('dwHighDateTime', ctypes.c_uint32)
    ]

st = SYSTEMTIME(1918, 4, 0, 13, 0, 0, 0, 0)
ft = FILETIME()
res = kernel32.SystemTimeToFileTime(ctypes.byref(st), ctypes.byref(ft))
print('stRef success:', res)

st2 = SYSTEMTIME()
kernel32.GetLocalTime(ctypes.byref(st2))
ft2 = FILETIME()
res2 = kernel32.SystemTimeToFileTime(ctypes.byref(st2), ctypes.byref(ft2))
print('stNow success:', res2)

uRef = (ft.dwHighDateTime << 32) | ft.dwLowDateTime
uNow = (ft2.dwHighDateTime << 32) | ft2.dwLowDateTime

print('diff in 100ns:', uNow - uRef)
print('days:', (uNow - uRef) // 864000000000)
