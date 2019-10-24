# DeepBayesIris

# Requirements:
ROOT: https://root.cern.ch/
BAT: https://github.com/bat/bat

# Installation

## Install ROOT (downloading binaries is recommended), and set it up:
```
source ${HOME}/local/root/bin/thisroot.sh
```

## Install BAT
```
./configure --prefix=$HOME/local
make
make install
```

Make sure the BAT libraries are available (follow instructions to set up environment variables)

## Compile DeepBayesIris

```
git clone https://github.com/rdisipio/DeepBayesIris.git
cd DeepBayesIris
make
```

## Training

```
./runDeepBayesIris
```

## Inference

Run:

```
./predict [eventId] [numberOfRuns]
```

Inspect output file:

```
root predict.root
root [0] 
Attaching file predict.root as _file0...
(TFile *) 0x7faac86e2830
root [0] output->Draw("lego")
```



