from pymongo import MongoClient
from datetime import timedelta, datetime, date
import json

def lastDayTemperatures(id):

	client = MongoClient('localhost', 27017)

	db = client.homeAware
	collection = db.temperatureSensors

	now = datetime.now()

	starttime = datetime.strptime(str(now.year) +  '-' +  str(now.month) + '-' + str(now.day) + '-' + str(now.hour), '%Y-%m-%d-%H')
	endtime = starttime - timedelta(hours = 1)

	temperatureDictionary = {}
	humidityDictionary = {}


	for x in range(24):

		summationTemperature = 0
		avgTemperature = 0.0

		summationHumidity = 0
		avgHumidity = 0.0

		cursor = collection.find({'id':id, "timestamp":{'$gte':endtime, '$lt':starttime}})

		starttime -= timedelta(hours = 1)
		endtime -= timedelta(hours = 1)

		for doc in cursor:
			summationTemperature += doc['temperature']
			summationHumidity += doc['humidity']

		if(cursor.count() == 0):
			avgTemperature = None;
			avgHumidity = None;

		else:

			avgTemperature = summationTemperature / float(cursor.count())
			avgHumidity = summationHumidity / float(cursor.count())

		temperatureDictionary["h" + str(x)] = avgTemperature
		humidityDictionary["h" + str(x)] = avgHumidity

		#print('Hour: ' + str(x) + '\n' + 'Average Temperature: ' + str(avgTemperature) + '\nAverage Humidity: ' + str(avgHumidity))
		
	json_string = json.dumps(temperatureDictionary)
	return json_string

if __name__ == '__main__':
	print(lastDayTemperature(str(123124)))

