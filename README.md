# Get Started

## convert runtime format from exchange format
```bash
/convert_runtime 09article < /home/irlab/exchange_file/09article.exch
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
