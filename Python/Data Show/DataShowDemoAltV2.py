import datetime
import os
import time
from termcolor import colored
import requests
import json

sensorIDs = ["123123", "bbf1c9"]

sensorIDToName = {
	"123123" : "Test Pub Script Sensor",
	"bbf1c9" : "Living Room Sensor"
}

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

				print("Sensor ID:\t" + sensorIDs[x])
				print("Sensor Name:\t" + sensorIDToName[sensorIDs[x]])

				print("")
				
				print("Temperature:\t" + str(latestData["temperature"]) + " " + u"\u00B0" + "C")
				print("Humidity:\t" + str(latestData["humidity"]) + "%")

				print("")

				if(latestData["statuscode"] == 3):
					print("Status:\t\t" + "Offline " + colored(u"\u25CF", 'red'))
				elif(latestData["statuscode"] == 2):
					print("Status:\t\t" + "Heartbeat Missing " + colored(u"\u25CF", 'yellow'))
				elif(latestData["statuscode"] == 1):
					print("Status:\t\t" + "Online " + colored(u"\u25CF", 'green'))				

			print("Last Updated:\t" + latestData["lastupdated"] + " ago")

		else:
			print("Unable to get data from the server. Response: " + str(r.status_code))
		

	print("")
	print('--------------------------------------------------------')
	print("")

	time.sleep(30)
	os.system('clear')



