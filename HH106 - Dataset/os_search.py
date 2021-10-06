#!/usr/bin/python
from datetime import datetime
import math 

#filename = 'hh106/hh106.annMATHIAS-original-edited-downsampled.txt'
#filename = 'hh106/hh106.annMATHIAS-original-edited-downsampled-firstday.txt'
#filename = 'hh106/hh106.annMATHIAS-original-edited-downsampled-outsidedoor.txt'
#filename = 'hh106/hh106.annMATHIAS-original-edited-downsampled-firstday-outsidedoor.txt'
filename = 'hh106/hh106.annMATHIAS-original-edited-downsampled-outsidedoor-no-last-day.txt'
allines = []
with open(filename) as file: 
    for line in file:
        allines.append(line.rstrip().split(" "))

rooms_occurrence = [[] for x in range( 8 ) ]

# for i in rooms_occurrence:
#     for j in i:
#         print(j)
#     print('\n')


for pair in allines:
    if pair[2] == 'WorkArea':
        rooms_occurrence[0].append(pair[1])
    if pair[2] == 'LivingRoom':
        rooms_occurrence[1].append(pair[1])
    if pair[2] == 'Kitchen':
        rooms_occurrence[2].append(pair[1])
    if pair[2] == 'Bedroom':
        rooms_occurrence[3].append(pair[1])
    if pair[2] == 'Chair':
        rooms_occurrence[4].append(pair[1])
    if pair[2] == 'DiningRoom':
        rooms_occurrence[5].append(pair[1])
    if pair[2] == 'Bathroom':
        rooms_occurrence[6].append(pair[1])
    if pair[2] == 'OutsideDoor':
        rooms_occurrence[7].append(pair[1])


request_hour_string = 22.0
twelve_hours = 12.0
count = 0
time_diff = 0.0
for i in rooms_occurrence:
    sum = 0.0
    for j in i:
        hours = j.split(":")
        time_diff = abs(int(hours[0]) - request_hour_string)
        if time_diff > 12:
            if request_hour_string < int(hours[0]):
                time_diff = abs(24 - int(hours[0])) + request_hour_string 
            else:
                time_diff = abs(24 - request_hour_string) + int(hours[0])   
        # print("Req:%d | Obj: %d | Time_diff: %d | Divi: %f | Curr. Val: %f | Sum: %f"%(request_hour_string, int(hours[0]), time_diff,(time_diff/twelve_hours), 1 - (time_diff/twelve_hours), sum))
        # time_diff = math.sqrt(time_diff)
        sum += (1 - abs(time_diff/twelve_hours))
    if count == 0:
        print('WorkArea:   '),
    if count == 1:
        print('LivingRoom: '),
    if count == 2:
        print('Kitchen:    '),
    if count == 3:
        print('Bedroom:    '),
    if count == 4:
        print('Chair:      '),
    if count == 5:
        print('Diningroom: '),
    if count == 6:
        print('Bathroom:   '),                                            
    if count == 7:
        print('OutsideDoor:'), 
    print(sum)
    count += 1



# FMT = '%H:%M:%S'
# request_hour_string = '13:00:00' 
# twelve_hours = 43200
# count = 0
# for i in rooms_occurrence:
#     sum = 0.0
#     for j in i:
#         time = datetime.strptime(j, FMT)
#         time_seconds = (time - datetime(1900,1,1)).seconds

#         request_hour = datetime.strptime(request_hour_string, FMT)
#         request_hour_seconds = (request_hour - datetime(1900,1,1)).seconds
#         # print(j, request_hour, twelve_hours, (time - request_hour).seconds)
#         time_diff = min((time - request_hour).seconds, (request_hour - time).seconds)
#         print("time: {} - {} | request: {} - {} | diff: {} | sum: {}".format(j, time_seconds, request_hour_string, request_hour_seconds, time_diff, sum))
#         sum += (1 - abs(time_diff/twelve_hours))
#     print(count, sum)
#     count += 1

# time1 = '23:00:00'
# time2 = '00:00:00'
# tfmt1 = datetime.strptime(time1, FMT)
# tfmt2 = datetime.strptime(time2, FMT)
# print((tfmt1 - tfmt2).seconds)

# with open(test, 'w') as outputfile: 
#     for pair in allines:
#         try:
#             # hours = pair.split(":")
#             # print(type(float(hours[0])))
#             # time = float(hours[0])*3600 + float(hours[1])*60 + float(hours[2])
#             # sum += 1 - abs(request_hour - time)/twelve_hours
#             # print(hours[0]+'-'+hours[1]+'-'+hours[2]+' | ')
#             # print(time)
#             outputfile.write(pair[0]+' '+pair[1]+' '+pair[2]+' '+pair[3]+'\n')
#         except:
#             print("ERROR")
# print(sum)

# days = []
# hours = []
# rooms = []
# days.clear()
# hours.clear()
# rooms.clear()

# day = -1
# hour = -1
# previous = 0
# for pair in allines:
#     if pair[2][0] == 'M':
#         date = pair[0].split("-")
#         if day != date[2]: #analysing the same day
#             if day != -1:
#                 days.append(hours)
#                 hours.clear()
#             day = date[2]
#         else:
#             hours = pair[1].split(":")
#             if hour != hours[0]:
#                 if hour != -1:
#                     hours.append(rooms)
#                     rooms.clear()
#                 hour = hours[0]
#             else:
#                 if previous == 0:
#                     previous = hours[0] * 360 + hours[1] * 60 + hours[2]
#                 else:
#                     current = hours[0] * 360 + hours[1] * 60 + hours[2]
#                     time_diff = current - previous
#                     previous = current
#                     if pair[3] == 'Bathroom':
#                         rooms[0]





#0 - Bathroom
#1 - Bedroom
#2 - Chair
#3 - DiningRoom
#4 - Hall
#5 - Ignore
#6 - Kitchen
#7 - LivingRoom
#8 - Office
#9 - OutsideDoor
#10 - WorkArea