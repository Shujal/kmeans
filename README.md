# kmeans
K-Means algorithm as Python Module.

###Install:
```bash
python setup.py build
sudo python setup.py install
```

###Uninstall:
```bash
sudo rm /usr/local/lib/python2.7/dist-packages/kmeans.so
```
*Not tested!*


###Usage:
Adds function kmeans.kmeans(points,k,delta).
Applys K-Means until max distance moved < delta.
```python
import kmeans
kmeans.kmeans([(5,11,13),(17,19,234),(1,1,1)],2,0.1)
```
