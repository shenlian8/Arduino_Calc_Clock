import json

minList = range(60)
minResult = {}

for i in range(60):
    minResult[f"{i:02d}"] = []

for n1 in range(100):
    for n2 in range(10):
        for n3 in range(10):
            
            # plus plus
            result = n1 + n2 + n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} + {n2} + {n3}")
                
            # minus plus
            result = n1 - n2 + n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} - {n2} + {n3}")
                        
            # multiplication plus
            result = n1 * n2 + n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} x {n2} + {n3}")
                
            # dividen plus
            if n2 != 0:
                result = n1 / n2 + n3
                if result in minList:
                    result = int(result)
                    minResult[f"{result:02d}"].append(f"{n1} / {n2} + {n3}")
                                
            # plus minus
            result = n1 + n2 - n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} + {n2} - {n3}")
                
            # minus minus
            result = n1 - n2 - n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} - {n2} - {n3}")
                        
            # multiplication minus
            result = n1 * n2 - n3
            if result in minList:
                minResult[f"{result:02d}"].append(f"{n1} x {n2} - {n3}")
                
            # dividen minus
            if n2 != 0:
                result = n1 / n2 - n3
                if result in minList:
                    result = int(result)
                    minResult[f"{result:02d}"].append(f"{n1} / {n2} - {n3}")


y = json.dumps(minResult)
print(y)