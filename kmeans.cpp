#include <Python.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <iostream>
#include <map>
using namespace std;


vector<pair<vector<int>,int> > kmeansInner(vector<pair<vector<int>, int> > points, int k, double delta){

	vector<
		pair<
			vector<double>, //coordinates
			vector<int> //points associated with this cluster
		>
	> clusters;

	vector<pair<vector<int>,int> >::iterator first=points.begin();
	vector<pair<vector<int>,int> >::iterator last=points.end();
	random_shuffle(first,last);
	vector<pair<vector<int>,int> >::iterator i=first;
	for(int counter=0;counter<k;i++,counter++){
		vector<double> coords;
		for(vector<int>::iterator j=i->first.begin();j!=i->first.end();j++){
			coords.push_back((double)(*j));
		}
		clusters.push_back(make_pair(coords,vector<int>()));
	}

	int n = points.size();
	int dim = points[0].first.size();
	while(1){
		for (int i = 0; i < k; i++) {
			clusters[i].second.clear();
		}
		for (int i = 0; i < n; i++) { // for each point

			double nearestDistance = numeric_limits<double>::infinity();
			int index = 0;
			
			for (int j = 0; j < k; j++) {
				
				double distance  = 0;

				for (int coord = 0; coord < dim; coord++) {
					
					double dx = points[i].first[coord]-clusters[j].first[coord];
					distance+=dx*dx;
					
				}
				
				if(distance<nearestDistance){
					nearestDistance = distance;
					index=j;
				}
				
			}

			
			clusters[index].second.push_back(i);
			
		}
		double maxDist = 0;
		for (int i = 0; i < k; i++) { // for each cluster
			
			vector<double> coords(dim); //new center
			int count=0;
			for(int coord = 0; coord < dim; coord++){
				count=0;
				for(vector<int>::iterator j=clusters[i].second.begin();j!=clusters[i].second.end();j++){
					coords[coord]+=points[*j].first[coord]*points[*j].second;
					count+=points[*j].second;
				}
				coords[coord]/=count;
			}
			double dist=0;
			for (int coord = 0; coord < dim; coord++) {
				double d=coords[coord]-clusters[i].first[coord];
				dist+=d*d;
				clusters[i].first[coord]=coords[coord];
			}
			clusters[i].second[0]=count;
			if(dist>maxDist)maxDist=dist;
		}

		if(maxDist<=delta)break;
	}

	vector<pair<vector<int>,int> > ans(k,make_pair(vector<int>(),0));
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < dim; j++) {
			ans[i].first.push_back((int)(clusters[i].first[j]));
		}
		ans[i].second=clusters[i].second[0];
	}
	return ans;

}

vector<pair<vector<int>,int> > kmeans(vector<vector<int> > points, int k, double delta){

	map<vector<int>,int> counter;
	for(vector<vector<int> >::iterator it=points.begin();it!=points.end();it++){
		counter[*it]++;
	}
	vector<pair<vector<int>,int> > countedPoints;
	for(map<vector<int>,int>::iterator it = counter.begin();it!=counter.end();it++){
		countedPoints.push_back(*it);
	}
	return kmeansInner(countedPoints,k,delta);
}





static PyObject* kmeans(PyObject* self, PyObject* args){
	
	PyObject* listPointer = NULL;
	int k=0;
	double delta=0;
	if(!PyArg_ParseTuple(args,"Oid",&listPointer,&k,&delta))return NULL;
	if(!PyList_Check(listPointer))return NULL;
	int n = PyList_Size(listPointer);
	if(n<=0)return NULL;
	vector<vector<int> > points(n,vector<int>()); 
	int dim;
	{
		PyObject* first=PyList_GetItem(listPointer,0);
		if(!PyTuple_Check(first))return NULL;
		dim=PyTuple_Size(first);
	}
	for (int i = 0; i < n; i++) {
		
		PyObject* point = PyList_GetItem(listPointer,i);
		
		if(!PyTuple_Check(point))return NULL;
		if(PyTuple_Size(point)!=dim)return NULL;
		
		for (int j = 0; j < dim; j++) {
			PyObject* coord = PyTuple_GetItem(point,j);
			if(!PyInt_Check(coord))return NULL;
			points[i].push_back(PyInt_AsLong(coord));
		}
		
	}
	vector<pair<vector<int>,int> > retVal = kmeans(points,k,delta);
	PyObject* ans = PyList_New(k);
	for (int i = 0; i < k; i++) {
		PyObject* element = PyTuple_New(2);
		PyObject* point = PyTuple_New(dim);
		PyObject* count = PyInt_FromLong(retVal[i].second);
		
		for (int j = 0; j < dim; j++) {
			PyObject* coord = PyInt_FromLong(retVal[i].first[j]);
			PyTuple_SetItem(point,j,coord);
		}
		PyTuple_SetItem(element,0,point);
		PyTuple_SetItem(element,1,count);
		PyList_SetItem(ans,i,element);
	}

	return Py_BuildValue("O",ans);

	


}


static PyMethodDef methods[] = {
{"kmeans",kmeans,METH_VARARGS, "Kmeans"},
{NULL,NULL,0,NULL}
};

PyMODINIT_FUNC initkmeans(){
	Py_InitModule("kmeans",methods);
}
