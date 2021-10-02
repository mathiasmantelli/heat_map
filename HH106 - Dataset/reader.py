#!/usr/bin/python
from datetime import datetime

process_raw_data = False

if process_raw_data:
    filename = 'hh106/hh106.annMATHIAS-original.txt'
    outputfilename = 'hh106/hh106.annMATHIAS-original-edited.txt'
    allines = []    
    #reading the dataset
    with open(filename) as file: 
        for line in file:
            allines.append(line.rstrip().split(" "))

    #writing the dataset after removing unnecessary data
    with open(outputfilename, 'w') as outputfile: 
        for pair in allines:
            try:
                if pair[2][0] == 'M':
                    hours = pair[1].split(":")
                    seconds = int(float(hours[2]))
                    current_time_string = hours[0]+':'+hours[1]+':'+str(seconds)                
                    outputfile.write(pair[0]+' '+current_time_string+' '+pair[3]+' '+pair[4]+' '+pair[2]+'\n')
            except:
                print("ERROR")
else:
    outputfilename = 'hh106/hh106.annMATHIAS-original-edited.txt'
    downsampled_outputfile = 'hh106/hh106.annMATHIAS-original-edited-downsampled.txt'
    allines = []

    #reading the cleaned dataset
    with open(outputfilename) as file: 
        for line in file:
            allines.append(line.rstrip().split(" "))


    daily_hours = [ 0 for y in range( 8 ) ]
    for j in range(8):
        daily_hours[j] = 0


    FMT = '%H:%M:%S'
    hours = allines[0][1].split(":")
    where = allines[0][2]
    current_hour = hours[0]
    previous_time_string = allines[0][1]

    i = 1
    with open(downsampled_outputfile, 'w') as outputfile: 
        while i < len(allines):
            hours = allines[i][1].split(":")
            current_time_string = hours[0]+':'+hours[1]+':'+hours[2]
            first_way_diff = datetime.strptime(current_time_string, FMT) - datetime.strptime(previous_time_string, FMT)
            second_way_diff = datetime.strptime(previous_time_string, FMT) - datetime.strptime(current_time_string, FMT)
            # if second_way_diff.seconds != first_way_diff.seconds:
            #     print(first_way_diff.seconds, second_way_diff.seconds, min(first_way_diff.seconds, second_way_diff.seconds))
            diff_time =  min(first_way_diff.seconds, second_way_diff.seconds)
            # print(when, where, diff_time)
            if where == 'WorkArea':
                daily_hours[0] += diff_time
            if where == 'LivingRoom':
                daily_hours[1] += diff_time
            if where == 'Kitchen':
                daily_hours[2] += diff_time
            if where == 'Bedroom':
                daily_hours[3] += diff_time
            if where == 'Chair':
                daily_hours[4] += diff_time
            if where == 'DiningRoom':
                daily_hours[5] += diff_time
            if where == 'Bathroom':
                daily_hours[6] += diff_time
            if where == 'OutsideDoor':
                daily_hours[7] += 0
            where = allines[i][2]
            previous_time_string = current_time_string
            if hours[0] != current_hour or i + 1 == len(allines):
                max_value = -1
                index = -1
                print(daily_hours[0], daily_hours[1], daily_hours[2], daily_hours[3], daily_hours[4], daily_hours[5], daily_hours[6], daily_hours[7])
                for j in range(8):
                    if daily_hours[j] > max_value:
                        max_value = daily_hours[j]
                        index = j
                    daily_hours[j] = 0
                room = "none"
                if index == 0:
                    room = 'WorkArea'
                if index == 1:
                    room = 'LivingRoom'
                if index == 2:
                    room = 'Kitchen'
                if index == 3:
                    room = 'Bedroom'
                if index == 4:
                    room = 'Chair'
                if index == 5:
                    room = 'DiningRoom'
                if index == 6:
                    room = 'Bathroom'
                if index == 7:
                    room = 'OutsideDoor'
                outputfile.write(allines[i][0]+' '+current_hour+':00:00 '+room+' '+allines[i][3]+' '+allines[i][4]+'\n')
                current_hour = hours[0]    
            i = i + 1