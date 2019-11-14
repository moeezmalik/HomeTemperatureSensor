from pymongo import MongoClient

client = MongoClient('104.154.165.28', 27017)

db = client.homeAware

temperatureSensors = db.temperatureSensors

bills_post = temperatureSensors.find({})
print(bills_post)

for doc in bills_post:
    print(doc)

    
