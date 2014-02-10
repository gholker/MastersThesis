## Master's Thesis

### Summary

This code was built to experiment with genetic programming and probabilistic models applied to neuroevolution. The full study is available here: http://digitalcommons.ryerson.ca/dissertations/1058/. The work covers several methodologies and the software can be configured to run any combination. The configurations used to generate the results of my thesis are provided in the ThesisResults folder.

### Abstract
This thesis proposed a methodology for the automatic design of neural networks via Estimation of Distribution Algorithms (EDA). The method evolves both topology and weights. To do so, topology is represented with a fixed-length, indirect encoding; weights are represented as a bitwise encoding. The topology and weights are searched via an incremental learning algorithm and a Guided Mutation operator. To explore suitable EDA ensembles, the study presented here interchangeably combined two representations for topology, two for weights, and two learning algorithms. Tests used in the analysis include: XOR, 6-bit Multiplexer, Pole-Balancing, and the Retina Problem. The results demonstrate that: (1) the Guided Mutation operator accelerated optimization on problems with a fixed fitness function; (2) the EDA approach introduced here is competitive with similar GP methods and is a viable method for Neuroevolution; (3) our methodology scales well to harder problems and automatically discovers modularity.


License
-------

__Simplified:__ http://www.tldrlegal.com/license/apache-license-2.0-(apache-2.0)

Copyright 2014 Graham Holker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.