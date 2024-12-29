---

## XML解析 liblist.xml
每添加或修改cvt node的时候，需要关注liblist.xml是否也需要修改

#### SINKTARGET
    最终格式，这个是显示格式

#### NODELIST
    根节点，包含所有的cvt node

#### NODELIST::NODE
    对应每个cvt node

#### NODELIST::NODE::lib
    对应每个cvt node最终生成的lib的名字，是lib的全名

#### NODELIST::NODE::source
    对应每个cvt node的输入格式

#### NODELIST::NODE::target
    对应每个cvt node的输出格式，输出格式要和输入格式对应，不然可能无法组成link关系

---

## 目录结构

- 每个目录对应一个格式转换
- private中存放私有协议

### RGB888

    RGB888 转 png

### YUV420SP
    
#### NV21

    YUV420SPNV21 转 RGB888
    格式：YYYYYYYYUVUV
    
#### NV12

    YUV420SPNV12 转 RGB888
    格式：YYYYYYYYVUVU
