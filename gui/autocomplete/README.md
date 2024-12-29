## .nyuv.config 配置说明

### FILTER

- 文件树的过滤器，用于屏蔽多余的文件类型
- 支持正则表达式
- 可修改

### PATH

- 存放程序的一些必要路径
- 暂时废弃，但是请不要随意修改

### FORMAT

- 用于自动补全、猜测RAW图的格式
- 每一个CLASS对应一个RAW图格式
- SIZE对因的是： height  width  stride  weight
- 请尽量不要修改weight熟悉，这是程序自动计算的，用于表示格式的权重（使用频繁度）