# DeepBayesIris

# Requirements:
ROOT: https://root.cern.ch/

BAT: https://github.com/bat/bat

# Installation

## Install ROOT 

Downloading binaries is recommended over compiling source files. 

To set it up:

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
root predict_0.root
root [0] 
Attaching file predict_0.root as _file0...
(TFile *) 0x7faac86e2830
root [0] output->Draw("lego")
```

## Make plots
To make nice-looking plots:

```
root
root [0] .L make_plot.C 
root [1] make_plot(1) // event id=1
root [2] make_plot(70) // event id=70
```


