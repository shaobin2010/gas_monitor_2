
import binascii
import json,base64

if __name__ == '__main__':
    # # 要编码的数据
    # data = {"uname":"Tanch","uid":3}
    # # 先转化为bytes类型数据
    # data_bytes = json.dumps(data).encode()
    # print(type(data_bytes))
    # # 编码
    # base_data = base64.b64encode(data_bytes)
    # print(base_data)

    # 解码
    string_bytes = "qlUAAGkjIjg2OTk3NTAzODE5NzI5MgIDABFWqAoFGQRe0REQbQs0WwoAC0G4AR8RH1PaFRZrAz5kDC//=="
    ori_data = base64.b64decode(string_bytes)
    
    # 字符串
    bin_str = binascii.b2a_hex(ori_data)
    print(bin_str)
    print(len(bin_str)//2)


'''
aa55
00006923  pkg_id
22        cmd_id
383639393735303338313937323932   imei

0203001156a80a0519045ed111106d0b345b0a000b41b8011f111f53da15166b033e64

0c2f
ff
'''
