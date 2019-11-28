from pymongo import MongoClient
from datetime import datetime, timedelta

client = MongoClient('localhost', 27017)

db = client.homeAware
temperatureSensors = db.temperatureSensors

currentTime = datetime.now()

receivedData = {"id" : "123124",
				"humidity" : 57,
				"temperature" : 23,
				"timestamp" : currentTime
				}

for x in range(24):

	if '_id' in receivedData:
		del receivedData['_id']

 	receivedData["id"] = "123124"
 	receivedData["humidity"] = 57
 	receivedData["temperature"] = 20
 	receivedData["timestamp"] = currentTime

 	result = temperatureSensors.insert_one(receivedData)
 	print('One post: {0}'.format(result.inserted_id))

 	currentTime = currentTime - timedelta(hours = 1)
