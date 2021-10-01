#!/usr/bin/python
from datetime import datetime

# filename = 'hh106/hh106.annMATHIAS-only-15-days.txt'
#filename = 'hh106/second_day_data.txt'
filename = 'hh106/trash.txt'
outputfilename = 'output.txt'
test = 'livingroom.txt'
allines = []
rooms = [0]*8
daily_hours = [ [ 0 for y in range( 8 ) ] for x in range( 24 ) ]
for i in range(24):
    for j in range(8):
        daily_hours[i][j] = 0

# for i in range(24):
#     print(daily_hours[i][0], daily_hours[i][1], daily_hours[i][2], daily_hours[i][3], daily_hours[i][4], daily_hours[i][5], daily_hours[i][6], daily_hours[i][7])

with open(filename) as file: 
    for line in file:
        allines.append(line.rstrip().split(" "))

# for pair in allines:
#     if pair[2][0] == 'M':
#         hours = pair[1].split(":")
#         if pair[3] == 'WorkArea':
#             daily_hours[int(hours[0])][0] += 1
#         if pair[3] == 'LivingRoom':
#             daily_hours[int(hours[0])][1] += 1
#         if pair[3] == 'Kitchen':
#             daily_hours[int(hours[0])][2] += 1
#         if pair[3] == 'Bedroom':
#             daily_hours[int(hours[0])][3] += 1
#         if pair[3] == 'Chair':
#             daily_hours[int(hours[0])][4] += 1
#         if pair[3] == 'DiningRoom':
#             daily_hours[int(hours[0])][5] += 1
#         if pair[3] == 'Bathroom':
#             daily_hours[int(hours[0])][6] += 1
#         if pair[3] == 'OutsideDoor':
#             daily_hours[int(hours[0])][7] += 1
FMT = '%H:%M:%S'
hours = allines[0][1].split(":")
where = allines[0][2]
when = int(hours[0])
previous_time = float(hours[0])*3600 + float(hours[1])*60 + float(hours[2])
previous_time_string = hours[0]+':'+hours[1]+':'+hours[2]
print(previous_time_string)

i = 1
while i < len(allines):
    hours = allines[i][1].split(":")
    current_time_string = hours[0]+':'+hours[1]+':'+hours[2]
    tdelta = datetime.strptime(current_time_string, FMT) - datetime.strptime(previous_time_string, FMT)
    # print(current_time_string, previous_time_string, tdelta)
    # diff_time = tdelta.seconds
    diff_time = 1
    # print(when, where, diff_time)
    if where == 'WorkArea':
        daily_hours[when][0] += diff_time
    if where == 'LivingRoom':
        daily_hours[when][1] += diff_time
    if where == 'Kitchen':
        daily_hours[when][2] += diff_time
    if where == 'Bedroom':
        daily_hours[when][3] += diff_time
    if where == 'Chair':
        daily_hours[when][4] += diff_time
    if where == 'DiningRoom':
        daily_hours[when][5] += diff_time
    if where == 'Bathroom':
        daily_hours[when][6] += diff_time
    if where == 'OutsideDoor':
        daily_hours[when][7] += diff_time
    when = int(hours[0])
    where = allines[i][2]
    previous_time_string = current_time_string
    i = i + 1

for i in range(24):
    print("%2d - WorkArea:%5d - Livingroom:%5d - Kitchen:%5d - Bedroom:%5d - Chair:%5d - Diningroom:%5d - Bathroom:%5d - Outsidedoor:%5d"%(i,daily_hours[i][0],daily_hours[i][1],daily_hours[i][2],daily_hours[i][3],daily_hours[i][4],daily_hours[i][5],daily_hours[i][6],daily_hours[i][7]))

for i in range(24):
    max = 0
    index = 0
    count_zeros = 0
    for j in range(8):
        if daily_hours[i][j] > max:
            max = daily_hours[i][j]
            index = j
        if daily_hours[i][j] == 0:
            count_zeros += 1
    if count_zeros == 8:
        index = -1
    if index == -1:
        print(str(i)+' - No motion data')
    if index == 0:
        print(str(i)+' - WorkArea')
    if index == 1:
        print(str(i)+' - LivingRoom')
    if index == 2:
        print(str(i)+' - Kitchen')
    if index == 3:
        print(str(i)+' - Bedroom')
    if index == 4:
        print(str(i)+' - Chair')
    if index == 5:
        print(str(i)+' - Diningroom')
    if index == 6:
        print(str(i)+' - Bathroom')                                            
    if index == 7:
        print(str(i)+' - OutsideDoor')        

