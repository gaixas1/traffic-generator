import subprocess
import os
import sys
import argparse
import time
import threading
import random

parser = argparse.ArgumentParser(description='Launch a set of apps.')

parser.add_argument('-i', metavar='instance', type=int, help='Initial flowId', required = True)
parser.add_argument('-s', metavar='SRC', help='Node name', required = True)
parser.add_argument('-d', metavar='DST', nargs='+', help='Destination nodes names', required = True)

args = parser.parse_args()

srcName = args.s
dsts = args.d
initialInstance = args.i

print(initialInstance, srcName, dsts)

#srcName = "A"
#initialInstance = 1
#dsts = ["A", "B"]

difs = ""
voiceC = 10
dataC = 2
videoC = 3

endTime = time.time() + 5 + 10 + 60 # current + wait for servers + minTestTime

def current_milli_time ():
	return int(round(time.time() * 1000))

def getLastIn (t):
	vals = t.split('\n')
	v = 0
	for i in range(0, len(vals)-1):
		try:
			v = int(vals[i])
		except:
			pass
	return v

def starClockServer(timeout) :
	call_str = "timeout " + str(timeout) + "s "
	call_str += "clocksincro"
	call_str += " -n " + srcName+"_clockserver"
	call_str += " -i 1"
	call_str += difs
	#print(call_str) ## Remove
	#return ## Remove
	process = subprocess.Popen(call_str, shell=True, stdout=subprocess.PIPE)  
	out, _ = process.communicate();
	
def getDestinationClockDif(dstName, i):
	call_str = "clocksincro"
	call_str += " -n " + srcName+"_clockclient"
	call_str += " -i " + str(i)
	call_str += " -m " + dstName+"_clockserver"
	call_str += " -j 1"
	call_str += " -M 50"
	call_str += " -S 20"
	call_str += " " + difs
	#print(call_str) ## Remove
	#return "0" ## Remove
	process = subprocess.Popen(call_str, shell=True, stdout=subprocess.PIPE)  
	out, _ = process.communicate();
	return str(out).replace("b", "").replace("'","").replace(r'\r\n', r"\n").replace(r'\n', "\n")
	
def starServer(timeout) :
	call_str = "timeout " + str(timeout) + "s "
	call_str += "server"
	call_str += " -n " + srcName+"_server"
	call_str += " -i 1"
	call_str += " -R 1000" # Interval duration 1 s
	call_str += " -L 10 " # Latency range 10ms
	call_str += difs
	#print(call_str) ## Remove
	#return ## Remove
	process = subprocess.Popen(call_str, shell=True, stdout=subprocess.PIPE)  
	out, _ = process.communicate();
	
def startVoice(dstName, i, cdif):
	call_str = "voice_client"
	call_str += " -n " + srcName+"_voice"
	call_str += " -i " +str(i)
	call_str += " -m " + dstName+"_server"
	call_str += " -j 1"
	#call_str += " -Q qos_voice.txt"
	call_str += " -R 1000" # Record interval duration 1 s
	call_str += " -t "+ str(cdif)
	call_str += " -I "+ str(i)
	call_str += " -e false"
	call_str += " -r false"
	call_str += " -w false" #Sleep wait
	call_str += " -p 800" #Min PDU
	call_str += " -P 1200" #Max PDU
	call_str += " -v 16000" #interval in ns :: 16ms = 62.5Hz
	call_str += " -D 60000" #Test duration in ms :: 60s
	call_str += " -o 3000" #Min ON duration # 3s
	call_str += " -O 10000" #Max ON duration # 10s
	call_str += " -f 5000" #Min OFF duration # 5s
	call_str += " -F 20000 " #Max OFF duration # 20s
	call_str += difs
	#print(call_str) ## Remove
	#return ## Remove
	process = subprocess.Popen(call_str, shell=True, stdout=subprocess.PIPE)  
	out, _ = process.communicate();
	
def startData(dstName, i, cdif):
	call_str = "data_client"
	call_str += " -n " + srcName+"_data"
	call_str += " -i " +str(i)
	call_str += " -m " + dstName+"_server"
	call_str += " -j 1"
	#call_str += " -Q qos_data.txt"
	call_str += " -R 1000" # Record interval duration 1 s
	call_str += " -t "+ str(cdif)
	call_str += " -I "+ str(i)
	call_str += " -e false"
	call_str += " -r false"
	call_str += " -w false" #Sleep wait (true => busy wait)
	call_str += " -p 1400" #Min PDU
	call_str += " -P 1400" #Max PDU
	call_str += " -v 560" #interval in ns :: Avg burst(5) = 2.8ms
	call_str += " -b 4" #Min burst size
	call_str += " -B 6" #Max burst size
	
	data_len = random.randrange(10000000, 50000000) # 10MB / 50MB
	icall_str = call_str + " -D " + str(data_len) + " " + difs
	#print(icall_str) ## Remove
	#return ## Remove
	
	while current_milli_time() < endTime:
		data_len = random.randrange(10000000, 50000000) # 10MB / 50MB
		icall_str = call_str + " -D " + str(data_len) + " " + difs
		process = subprocess.Popen(icall_str, shell=True, stdout=subprocess.PIPE)  
		out, _ = process.communicate();
		time.sleep(random.randrange(5000, 10000) / 1000.0) # 5 - 10 s
		
	
def startVideo(dstName, i, cdif):
	video_len = 1000
	
	f = open(dstName + "_"+ str(i)+'.video.stats', 'w')

	call_str = "data_client"
	call_str += " -n " + srcName+"_video"
	call_str += " -i " +str(i)
	call_str += " -m " + dstName+"_server"
	call_str += " -j 1"
	#call_str += " -Q qos_video.txt"
	call_str += " -R 1000" # Interval duration 1 s
	call_str += " -t "+ str(cdif)
	call_str += " -I "+ str(i)
	call_str += " -e false"
	call_str += " -r false"
	call_str += " -w false" #Sleep wait
	call_str += " -p 1400" #Min PDU
	call_str += " -P 1400" #Max PDU
	call_str += " -v 2240" #interval in ns :: 2.24ms = ~445Hz
	call_str += " -b 1" #Min burst size
	call_str += " -B 1" #Max burst size
	
	#data_len = random.randrange(5000000, 7500000) # 5MB / 7.5MB
	#icall_str = call_str + " -D " + str(data_len) + " " + difs
	#print(icall_str) ## Remove
	#return ## Remove
	
	while current_milli_time() < endTime:
		t0 = current_milli_time()
		data_len = random.randrange(5000000, 7500000) # 5MB / 7.5MB
		icall_str = call_str + " -D " + str(data_len) + " " + difs
		process = subprocess.Popen(icall_str, shell=True, stdout=subprocess.PIPE)  
		out, _ = process.communicate();
		t1 = current_milli_time()
		dt = t1 - t0
		if dt < 2000 : # 2s
			time.sleep((2-dt) / 1000.0)
			f.write("OK;" + str(dt) + ";"+str(data_len)+ "\n")
		else :
			f.write("KO;" + str(dt) + ";"+str(data_len)+ "\n")
		f.close()
	
def testDestination(dstName, i):	
	T = []
	
	#start server
	t = threading.Thread(target=starServer, args=(10 + 60 + 20,)) # timeout after test_time + testDuration + 20
	T.append(t)
	t.start()
	
	#get clock time difference between client and server node
	cdif = getLastIn(getDestinationClockDif("B", i))
	print (cdif)
	time.sleep(10) ## replace with sleep until init_time + test_time

	##Start voice, data and video clients v, D, V 
	instance = initialInstance + i * (voiceC + dataC + videoC)
	
	#Start voice flows
	for i in range(0, voiceC):
		t = threading.Thread(target=startVoice, args=(dstName, instance, cdif))
		T.append(t)
		t.start()
		instance = instance+1
		
	#Start data flows
	for i in range(0, dataC):
		t = threading.Thread(target=startData, args=(dstName, instance, cdif))
		T.append(t)
		t.start()
		instance = instance+1
		
	#Start video flows
	for i in range(0, dataC):
		t = threading.Thread(target=startVideo, args=(dstName, instance, cdif))
		T.append(t)
		t.start()
		instance = instance+1
	
	#Wait until all threads have ended
	for t in T:
		t.join()
		
	print("END test ", dstName)
	
		

T = []

#Start clock server
t = threading.Thread(target=starClockServer, args=(30,))
T.append(t)
t.start()

#Wait for clock server to be online
time.sleep(5)

#start flows to each destination
i = 0
for dstName in dsts:
	t = threading.Thread(target=testDestination, args=(dstName,i))
	T.append(t)
	t.start()
	i = i+1

#Wait until all threads have ended
for t in T:
	t.join()
	
print("END")