import datetime
import os
import time
from termcolor import colored
import requests
import json


def secondsToSignificant(inputSeconds):

    outputString = "N"

    if(inputSeconds > 0):

        if(inputSeconds <60):
            outputString =  str(int(inputSeconds)) + "s"
        elif(inputSeconds < 3600):
            minutes = round(inputSeconds/60)
            outputString = str(int(minutes)) + "m"
        elif(inputSeconds<86400):
            hours = round(inputSeconds/3600)
            outputString = str(int(hours)) + "h"
        elif(inputSeconds<31536000):
            days = round(inputSeconds/86400)
            outputString = str(int(days)) + "d"
        else:
            years = round(inputSeconds/31536000)
            outputString = str(int(years)) + "y"

    return outputString

sensorIDs = ["123123", "bbf1c9"]

sensorIDToName = {
	"123123" : "Test Pub Script Sensor",
	"bbf1c9" : "Living Room Sensor"
}

###########################################
########### CONNECT TO MONGO DB ###########
###########################################

os.system('clear')



while(True):

	for x in range(len(sensorIDs)):

		r = requests.get("http://gcpserverip/request/simple?id=" + sensorIDs[x])

		print("")
		print('--------------------------------------------------------')
		print("")
		
		if(r.status_code == 200):

			latestData = json.loads(r.content)

			if(latestData == None):
				print("No sensor found for ID: " + sensorIDs[x])

			else:
				timeStampData = latestData["epochtimestamp"]
				timeStampNow = (datetime.datetime.utcnow() - datetime.datetime(1970,1,1)).total_seconds()

				timeDifferenceSeconds = timeStampNow-timeStampData

				print("Sensor ID:\t" + sensorIDs[x])
				print("Sensor Name:\t" + sensorIDToName[sensorIDs[x]])

				print("")
				
				print("Temperature:\t" + str(latestData["temperature"]) + " " + u"\u00B0" + "C")
				print("Humidity:\t" + str(latestData["humidity"]) + "%")

				print("")

				if(timeDifferenceSeconds > 600):
					print("Status:\t\t" + "Offline " + colored(u"\u25CF", 'red'))
				elif(timeDifferenceSeconds > 30):
					print("Status:\t\t" + "Heartbeat Missing " + colored(u"\u25CF", 'yellow'))
				else:
					print("Status:\t\t" + "Online " + colored(u"\u25CF", 'green'))				

			print("Last Updated:\t" + secondsToSignificant(timeDifferenceSeconds) + " ago")

		else:
			print("Unable to get data from the server. Response: " + str(r.status_code))
		

	print("")
	print('--------------------------------------------------------')
	print("")

	time.sleep(30)
	os.system('clear')



