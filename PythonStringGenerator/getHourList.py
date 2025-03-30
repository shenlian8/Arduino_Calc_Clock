import json

hourList = range(24)
hourResult = {}

for i in range(24):
    hourResult[f"{i:02d}"] = []

for n1 in range(2, 13):
    for n2 in range(2, 13):
        
        # plus
        result = n1 + n2
        if result in hourList:
            hourResult[f"{result:02d}"].append(f"{n1} + {n2}")
            #print (f"{n1} + {n2} = {result}")
            
        # minus
        result = n1 - n2
        if result in hourList:
            hourResult[f"{result:02d}"].append(f"{n1} - {n2}")
            #print (f"{n1} - {n2} = {result}")
                    
        # multiplication
        result = n1 * n2
        if result in hourList:
            hourResult[f"{result:02d}"].append(f"{n1} x {n2}")
            #print (f"{n1} x {n2} = {result}")
            
        # dividen
        """
        if n2 != 0:
            result = n1 / n2
            if result in hourList:
                result = int(result)
                hourResult[f"{result:02d}"].append(f"{n1} / {n2}")
                #print (f"{n1} / {n2} = {result}")
        """

y = json.dumps(hourResult)
print(y)