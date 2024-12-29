FROM daocloud.io/library/ubuntu:16.04 AS U16
VOLUME /nyuv/
RUN apt update && apt install -y make cmake gcc build-essential qt5-default
WORKDIR /nyuv/
CMD ["/bin/bash", "-c", "mkdir u16 && cd ./u16/ && rm -fr ./* && cmake -DINSTALL=ON .. && make -j8 && make -j8 package"]

FROM daocloud.io/library/ubuntu:18.04 AS U18
VOLUME /nyuv/
RUN apt update && apt install -y make cmake gcc build-essential qt5-default
WORKDIR /nyuv/
CMD ["/bin/bash", "-c", "mkdir u18 && cd ./u18/ && rm -fr ./* && cmake -DINSTALL=ON .. && make -j8 && make -j8 package"]