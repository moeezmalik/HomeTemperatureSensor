inputSeconds = 151536000

def secondsToSignificant(inputSeconds):

    outputString = "N"

    if(inputSeconds > 0):

        if(inputSeconds <60):
            outputString =  str(inputSeconds) + "s"
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

print(secondsToSignificant(inputSeconds))