## Hotcall
Hotcall Version of NestedSGX.

We've applied Hotcall strategy to the linux-sgx and occlum as well. These two components can be cooperated together to accelerate the speed of fio/redis.

The basic idea can be found in Hotcall[ISCA'17], with a very good implementation in [github](https://github.com/oweisse/hot-calls).

Since in occlum, we not only change the source code of occlum, but also the dependencies of occlum (like rust-sgx-sdk) as well. I've tried several times to upload all of them but it always miss something, so now I choose to upload a whole `.tar.gz` here.

## Usage:
Make sure you've in the docker image of occlum.
- in linux-sgx, please use `make clean && make sdk_install_pkg_no_mitigation` to get the `binary` setup package.
- run `./sgx_linux_x64_sdk_2.20.100.4.bin` to install sgx-sdk, make sure you always install it in `/opt/intel`, after that, use `source /opt/intel/sgxsdk/environment` to apply it in the OS.
- unzip the `occlum_hotcall.tar.gz`, in occlum dirent, run `make submodule && SGX_MODE=SIM make && make install`

## Others:
Our implementation use wild optimization, so it might fail to run `fio/redis` as we mentioned in `ae` repo, you might need to try several times to get the data you want.

The data of `fio/redis` we've shown in our paper is based on hotcall-version results.

Let's Rock n Roll!