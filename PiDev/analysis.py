import os
import numpy


def open_file(filename):
    with open(filename) as f:
        return f.readlines()

def print_out_summary(pulse_array, timestamp):
    print "======================"
    print timestamp
    print "mean pulse: " + str(numpy.mean(pulse_array))
    print "median pulse: " + str(numpy.median(pulse_array))
    print "======================"

def analyse_pulse(pulse_file):
    lines = open_file(pulse_file)
    pulse_array = []
    timestamp = ""
    for line in lines:
        if len(timestamp) == 0:
            timestamp = line.split(",")[0]
        if "2015" in line:
            pulse = int(line.split(",")[-1])
            pulse_array.append(pulse)
    print_out_summary(pulse_array, timestamp)

def analyse_all_pulse():
    main_dir = "./pics/"
    dirs = os.listdir(main_dir)
    root_path = os.path.abspath(os.path.dirname(main_dir))
    for f in dirs:
        analyse_pulse(root_path + "/" + f + "/" + "PULSE.TXT")


analyse_all_pulse()
