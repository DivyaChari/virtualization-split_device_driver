from boto.s3.connection import S3Connection
from boto.s3.key import Key
import socket
from kvparam_pb2 import kvparam

class S3KeyStore(object):
    def __init__(self):
        self.conn = S3Connection('AKIAJRC346UMO4WSKGQQ', 'yqcm5db+n8Lqf+Sw49qSHbLCXjslT7JV+z9AJBQI')
        self.bucket = self.conn.create_bucket('virtproject3')
        
    def get(self, key):
        k = Key(self.bucket)
        k.key = key
	return k.get_contents_as_string()
        
    def put(self, key, value):
        k = Key(self.bucket)
        k.key = key
	try:
		k.set_contents_from_string(value)
		return 0
	except Exception:
		return -1

if __name__ == "__main__":
	key_value_store = S3KeyStore()
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind(("127.0.0.1", 5555))

	kv = kvparam()
	while True:
		data, addr = sock.recvfrom(1024)
    		kv.ParseFromString(data)
		if kv.operation == 1:
			if key_value_store.bucket.get_key(kv.key) is None:
				print("Key does not exist in bucket")
				kv.ret = -1
			else:
				value = key_value_store.get(kv.key)
				length = len(value)
				if length > 4096:
					print("Data length exceeds limits")
					kv.ret = -2
				else:
					kv.value = value
					kv.length = length
					kv.ret = 0
					print("Get operation with key={0} returned value={1}".format(kv.key, kv.value))
			reply = kv.SerializeToString()
			sock.sendto(reply, addr)
		if kv.operation == 2:
			if len(kv.value) == 0:
				key_value_store.bucket.delete_key(kv.key)
				print("Key {0} deleted".format(kv.key))
				kv.ret = 0
			else:
				kv.ret = key_value_store.put(kv.key, kv.value)
				print("Key {0} updated with value {1}".format(kv.key, kv.value))
			reply = kv.SerializeToString()
			sock.sendto(reply, addr)
