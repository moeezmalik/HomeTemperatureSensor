#This is the python script for Flask RESTful API

#!flask/bin/python

from flask import jsonify
from flask import Flask, request
import pymongo
from pymongo import MongoClient
import datetime

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

def statusCode(secondsSinceLastUpdate):
    if(secondsSinceLastUpdate > 600):
        return 3
    elif(secondsSinceLastUpdate > 30):
        return 2
    else:
        return 1 

##############################################
########### CONNECTION TO PYMONGO ############
##############################################

client = MongoClient('gcpserverip', 27017)
db = client.homeAware
tsCollection = db.temperatureSensors

serializableData = {"temperature" : "12"}

##############################################
############ FLASK IMPLEMENTATION ############
##############################################

app = Flask(__name__)

@app.route('/request/simple', methods=['GET'])
def user():

    data = request.args
    print(data)

    latestData = tsCollection.find_one({'id': data['id']},sort=[( '_id', pymongo.DESCENDING )])

    if(latestData == None):
    	return ""
    else:

	    print(latestData)

	    secondsSinceLastUpdate = (datetime.datetime.utcnow() - latestData["timestamp"]).total_seconds()
	    
	    serializableData["temperature"] = latestData["temperature"]
	    serializableData["humidity"] = latestData["humidity"]
	    serializableData["id"] = latestData["id"]
	    serializableData["lastupdated"] = secondsToSignificant(secondsSinceLastUpdate)
	    serializableData["statuscode"] = statusCode(secondsSinceLastUpdate)

	    return jsonify(serializableData)
    

if __name__ == "__main__":
    app.run()
