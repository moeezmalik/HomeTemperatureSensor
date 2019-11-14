#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (c) 2016 Roger Light <roger@atchoo.org>
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Distribution License v1.0
# which accompanies this distribution.
#
# The Eclipse Distribution License is available at
#   http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Roger Light - initial implementation

# This shows an example of using the subscribe.callback helper function.

import context  # Ensures paho is in PYTHONPATH
import paho.mqtt.subscribe as subscribe
import json
from pymongo import MongoClient
import datetime

client = MongoClient('localhost', 27017)

db = client.homeAware
temperatureSensors = db.temperatureSensors

def print_msg(client, userdata, message):

    receivedData=json.loads(message.payload)

    print("ID: " + receivedData["id"])
    print("Temperature: " + str(receivedData["temperature"]))
    print("humidity: " + str(receivedData["humidity"]))

    receivedData["timestamp"] = datetime.datetime.now()

    result = temperatureSensors.insert_one(receivedData)
    print('One post: {0}'.format(result.inserted_id))



subscribe.callback(print_msg, "home/things/sensors/temperature", hostname="localhost")