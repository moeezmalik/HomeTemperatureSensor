import pymongo
from pymongo import MongoClient
import datetime
import os
import time
from termcolor import colored

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

client = MongoClient('gcpserverip', 27017)
db = client.homeAware
tsCollection = db.temperatureSensors

while(True):

	for x in range(len(sensorIDs)):

		latestData = tsCollection.find_one({'id': sensorIDs[x]},sort=[( '_id', pymongo.DESCENDING )])

		print("")
		print('--------------------------------------------------------')
		print("")

		if(latestData == None):
			print("No sensor found for ID: " + sensorIDs[x])

		else:
			timeStampData = latestData["timestamp"]
			timeStampNow = datetime.datetime.utcnow()

			timeDifference = timeStampNow-timeStampData
			timeDifferenceSeconds = timeDifference.total_seconds()

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
		

	print("")
	print('--------------------------------------------------------')
	print("")

	time.sleep(30)
	os.system('clear')



