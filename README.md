# Get Started
## Docker Hub Info
https://hub.docker.com/r/devopsifyinc/okapi/
## Run Docker
```
docker run -it devopsifyinc/okapi bash
```
## convert runtime format from exchange format
```bash
/convert_runtime 09article < /home/okapi/input/Chem_para_1000.exch
```

## convert index
```bash
./ix1 -mem 500 -delfinal -doclens 09article 0 | ./ixf 09article 0
```

## run sample program
```bash
source environmentSettings.bshrc
cd $OKAPI_JAVABIN
java searching
```
