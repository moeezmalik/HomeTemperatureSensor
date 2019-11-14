#This is the python script for Flask RESTful API

#!flask/bin/python

from flask import jsonify
from flask import Flask, request
import pymongo
from pymongo import MongoClient
import datetime

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

    print(latestData)
    
    serializableData["temperature"] = latestData["temperature"]
    serializableData["humidity"] = latestData["humidity"]
    serializableData["id"] = latestData["id"]
    serializableData["epochtimestamp"] = (latestData["timestamp"]-datetime.datetime(1970, 1,1)).total_seconds()

    return jsonify(serializableData)
    

if __name__ == "__main__":
    app.run()
