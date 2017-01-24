filename = "testdata.csv"
numlines = 500
separator = ","

import random

file = open("testdata.csv","w")

transIdGen = list("00000000")
userIdGen = list("00000000")
usedUserId = list()
bankIdGen = list("00000000")
usedBankId = list()
ssnGen = list("00000000")
usedSsn = list()
cellGen = list("0000000000")
usedCell = list()
ipGen = list("00000000")
usedIp = list()
imeiGen = list("00000000")
usedImei = list()

def incrementTransIdGen():
  global transIdGen
  ind = len(transIdGen) - 1
  while (True):
    if (transIdGen[ind] == "9"):
      transIdGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many transactions made.  Ids are being repeated as a result."
    else:
      transIdGen[ind] = chr(ord(transIdGen[ind]) + 1)
      break
  return
def incrementUserIdGen():
  global userIdGen
  ind = len(userIdGen) - 1
  while (True):
    if (userIdGen[ind] == "9"):
      userIdGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many users made.  Ids are being repeated as a result."
    else:
      userIdGen[ind] = chr(ord(userIdGen[ind]) + 1)
      break
  return
def incrementBankIdGen():
  global bankIdGen
  ind = len(bankIdGen) - 1
  while (True):
    if (bankIdGen[ind] == "9"):
      bankIdGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many banks made.  Ids are being repeated as a result."
    else:
      bankIdGen[ind] = chr(ord(bankIdGen[ind]) + 1)
      break
  return
def incrementSsnGen():
  global ssnGen
  ind = len(ssnGen) - 1
  while (True):
    if (ssnGen[ind] == "9"):
      ssnGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many SSNs made.  Ids are being repeated as a result."
    else:
      ssnGen[ind] = chr(ord(ssnGen[ind]) + 1)
      break
  return
def incrementCellGen():
  global cellGen
  ind = len(cellGen) - 1
  while (True):
    if (cellGen[ind] == "9"):
      cellGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many cells made.  Ids are being repeated as a result."
    else:
      cellGen[ind] = chr(ord(cellGen[ind]) + 1)
      break
  return
def incrementImeiGen():
  global imeiGen
  ind = len(imeiGen) - 1
  while (True):
    if (imeiGen[ind] == "9"):
      imeiGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many IMEIs made.  Ids are being repeated as a result."
    else:
      imeiGen[ind] = chr(ord(imeiGen[ind]) + 1)
      break
  return
def incrementIpGen():
  global ipGen
  ind = len(ipGen) - 1
  while (True):
    if (ipGen[ind] == "9"):
      ipGen[ind] = "0"
      ind-=1
      if (ind < 0):
        print "Too many IPs made.  Ids are being repeated as a result."
    else:
      ipGen[ind] = chr(ord(ipGen[ind]) + 1)
      break
  return

for i in xrange(numlines):
  #trans_id
  file.write(''.join(transIdGen) + separator)
  incrementTransIdGen()

  #trans_vol
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #status
  if (random.randrange(0,20) != 0):
    file.write(chr(ord('A') + random.randrange(0,26)))
  file.write(separator)

  #dt_create
  if (random.randrange(0,20) != 0):
    file.write('DATE')
  file.write(separator)

  #score
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #case_id
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #oid_partner
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #category
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #user_id
  if (random.randrange(0,20) != 0):
    if (random.randrange(0,50) >= min(39,len(usedUserId))): #New user
      file.write(''.join(userIdGen))
      usedUserId.append(''.join(userIdGen))
      incrementUserIdGen()
    else:
      file.write(usedUserId[random.randrange(0,len(usedUserId))])
  file.write(separator)

  #reg_date
  if (random.randrange(0,20) != 0):
    file.write('DATE')
  file.write(separator)

  #user_login
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #bank_cardno
  if (random.randrange(0,20) != 0):
    if (random.randrange(0,50) >= min(39,len(usedBankId))): #New user
      file.write(''.join(bankIdGen))
      usedBankId.append(''.join(bankIdGen))
      incrementBankIdGen()
    else:
      file.write(usedBankId[random.randrange(0,len(usedBankId))])
  file.write(separator)

  #bank_cardtype
  if (random.randrange(0,20) != 0):
    file.write(chr(ord('0') + random.randrange(0,3)))
  file.write(separator)

  #bankcode
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #bank_phone
  if (random.randrange(0,20) != 0):
    if (random.randrange(0,50) >= min(39,len(usedCell))): #New user
      file.write(''.join(cellGen))
      usedCell.append(''.join(cellGen))
      incrementCellGen()
    else:
      file.write(usedCell[random.randrange(0,len(usedCell))])
  file.write(separator)

  #bank_idno && bank_idtype
  if (random.randrange(0,20) != 0):
    bank_idtype = chr(ord('0') + random.randrange(10))
    if (bank_idtype == '0' or bank_idtype == '7'):
      if (random.randrange(0,20) != 0):
        if (random.randrange(0,50) >= min(39,len(usedSsn))): #New user
          file.write(''.join(ssnGen))
          usedSsn.append(''.join(ssnGen))
          incrementSsnGen()
        else:
          file.write(usedSsn[random.randrange(0,len(usedSsn))])
    else:
      file.write(str(random.randrange(0,1001)))
    file.write(separator)
    file.write(bank_idtype)
  else:
    file.write(separator)
  file.write(separator)

  #bank_name
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #ip
  if (random.randrange(0,20) != 0):
    if (random.randrange(0,50) >= min(39,len(usedIp))): #New user
      file.write(''.join(ipGen))
      usedIp.append(''.join(ipGen))
      incrementIpGen()
    else:
      file.write(usedIp[random.randrange(0,len(usedIp))])
  file.write(separator)

  #mac_addr
  if (random.randrange(0,20) != 0):
    file.write(str(random.randrange(0,1001)))
  file.write(separator)

  #imei
  if (random.randrange(0,20) != 0):
    if (random.randrange(0,50) >= min(39,len(usedImei))): #New user
      file.write(''.join(imeiGen))
      usedImei.append(''.join(imeiGen))
      incrementImeiGen()
    else:
      file.write(usedImei[random.randrange(0,len(usedImei))])
  file.write(separator)

  #biz_category
  if (random.randrange(0,20) != 0):
    file.write(chr(ord('A') + random.randrange(0,26)) + chr(ord('A') + random.randrange(0,26)))
  file.write(separator)

  #biz_code
  if (random.randrange(0,20) != 0):
    file.write(chr(ord('A') + random.randrange(0,26)) + chr(ord('A') + random.randrange(0,26)))
  file.write(separator)

  #type_pay
  if (random.randrange(0,20) != 0):
    file.write(chr(ord('A') + random.randrange(0,26)) + chr(ord('A') + random.randrange(0,26)))

  file.write('\n')

file.close()
