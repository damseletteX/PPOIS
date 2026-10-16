

class TransportCompany():
    def __init__(self):
        self.boss = CEO()
        self.industrialEnterprise = Enterprise()
        
    
'''CLASSES DESCRIBING DEPARTMENTS'''
class Enterprise():
    pass

class Office():
    def __init__(self):
        self.boss = ()
        
        
class HiringDepartment(Office):
    pass

class AccountingDepartment(Office):
    pass

'''HUMAN-RELATED CLASSES'''
class Employee():
    pass

'''Abstract class to describe features of IT specialists'''
class ITspecialist(Employee):
    pass

class Manager(Employee):
    pass

class ProductManager(Manager):
    pass

class ProjectManager(Manager):
    pass

class CEO(Employee):
    pass

class Engineer(Employee):
    pass

class MechanicalEngineer(Engineer):
    pass

class ElectricalEngineer(Engineer):
    pass

class SoftwareEngineer(Engineer, ITspecialist):
    pass

class DevOps(ITspecialist):
    pass

class SMM(Manager, ITspecialist):
    pass

class Cleaner(Employee):
    pass

class HR(Employee):
    pass

class Accountant(Employee):