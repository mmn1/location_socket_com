import socket
import sys
import json
import socket
from datetime import datetime


if __name__ == "__main__":
    # data = ' '.join(sys.argv[1:])
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(('127.0.0.1', 8001))
        # print("client {0} 发送 {1}".format(datetime.now(), data))
        # sock.sendall(bytes(data, "utf-8"))
        #########################################
        dict = {}
        dict['name'] = 'many'
        dict['age'] = 10
        dict['sex'] = 'male'
        print(dict)  # 输出：{'name': 'many', 'age': 10, 'sex': 'male'}
        send_msg = json.dumps(dict)
        # 发送结构体数组
        # send_msg = json.dumps({'a': 'str', 'c': True, 'e': 10, 'b': 11.1,
        #                        'd': None, 'f': 1, 'g': 2})
        sock.sendall(bytes(send_msg, "utf-8"))
        print(send_msg)

        # received_data = sock.recv(1024)
        '''获取报头数据，bytes'''
        b_hander = sock.recv(602400)
        print(1)
        # print(b_hander)

        '''报头数据解码 bytes-》str'''
        print(2)
        json_hander = b_hander.decode('utf-8')
        # print(json_hander)

        '''报头数据反序列化 str-》dict'''
        print(3)
        hander = json.loads(json_hander)
        print(hander)

        '''获取报头字典，取的文件长度，取出文件内容'''
        file_size = hander['len']
        print(4)
        print(file_size)

