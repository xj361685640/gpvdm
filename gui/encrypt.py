from Crypto.Cipher import AES
import hashlib
from inp import inp_get_token_value
iv=""
key=""

def encrypt_load():
	global key
	global iv
	iv=inp_get_token_value("crypto.inp","#iv")
	key=inp_get_token_value("crypto.inp","#key")

def encrypt(data):
	global key
	global iv
	ret=""
	m = hashlib.md5()
	m.update(key)
	key_hash=m.digest()

	m = hashlib.md5()
	m.update(iv)
	iv_hash=m.digest()

	encryptor = AES.new(key_hash, AES.MODE_CBC, IV=iv_hash)

	ret= encryptor.encrypt(bytes(data[0:512]))

	encryptor = AES.new(key_hash, AES.MODE_CBC, IV=iv_hash)

	ret=ret+encryptor.encrypt(bytes(data[512:len(data)]))

	return ret


def decrypt(data):
	global key
	global iv
	ret=""
	m = hashlib.md5()
	m.update(key)
	key_hash=m.digest()

	m = hashlib.md5()
	m.update(iv)
	iv_hash=m.digest()

	encryptor = AES.new(key_hash, AES.MODE_CBC, IV=iv_hash)

	ret= encryptor.decrypt(bytes(data))

	return ret
