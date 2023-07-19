#生成BMP图像
class BMP:
    def __init__(self, w, h):
        self.w = w
        self.h = h
        self.dataSize = h * w * 3
        self.rgbData = [[(0, 0, 0)] * w for _ in range(h)]  # 初始化RGB数据

    def _get_header_bytes(self):
        # 创建BMP头部的字节流
        header = bytearray(b'BM')
        header.extend((self.w * self.h * 3 + 54).to_bytes(4, 'little'))
        header.extend(b'\x00\x00\x00\x00')
        header.extend((54).to_bytes(4, 'little'))
        header.extend((40).to_bytes(4, 'little'))
        header.extend(self.w.to_bytes(4, 'little'))
        header.extend(self.h.to_bytes(4, 'little'))
        header.extend((1).to_bytes(2, 'little'))
        header.extend((24).to_bytes(2, 'little'))
        header.extend(b'\x00\x00\x00\x00')
        header.extend((self.w * self.h * 3).to_bytes(4, 'little'))
        header.extend(b'\x00\x00\x13\x0B')
        header.extend(b'\x00\x00\x13\x0B')
        header.extend(b'\x00\x00\x00\x00')
        header.extend(b'\x00\x00\x00\x00')

        return header

    def _get_rgb_data_bytes(self):
        min_temperature = float('inf')
        max_temperature = float('-inf')

        # 找到数据中的最大和最小温度
        for r in range(self.h):
            for c in range(self.w):
                temperature = self.rgbData[r][c]
                min_temperature = min(min_temperature, temperature)
                max_temperature = max(max_temperature, temperature)

        # 将RGB数据转换为字节流
        data = bytearray()
        zero_bytes = self.dataSize // self.h - self.w * 3
        for r in reversed(range(self.h)):  # 反向遍历行
            for c in range(self.w):
                temperature = self.rgbData[r][c]

                # 将温度映射为RGB值（根据动态的最大和最小温度）
                red = int(((temperature - min_temperature) / (max_temperature - min_temperature)) * 255)
                green = 0  # 固定绿色分量
                blue = int(((max_temperature - temperature) / (max_temperature - min_temperature)) * 255)

                # 写入颜色分量到字节流
                data.extend(bytes([blue, green, red]))

            # 写入零字节以对齐每行的字节数为4的倍数
            for _ in range(zero_bytes):
                data.extend(bytes([0x00]))

        return data

    def save_image(self, filename):
        # 将BMP保存为图片文件
        header = self._get_header_bytes()
        rgb_data = self._get_rgb_data_bytes()
        
        with open(filename, 'wb') as f:
            f.write(header)
            f.write(rgb_data)

    def get_binary_code(self):
        # 获取BMP图像的二进制数据流
        header = self._get_header_bytes()
        rgb_data = self._get_rgb_data_bytes()
        binary_code = bytes(header + rgb_data)
        return binary_code