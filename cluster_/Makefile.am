zip=zip


all: cluster.zip
	echo "cluster zipped"


cluster.zip: ../gpvdm
	cd ../cluster;$(zip) -rq ../cluster_/cluster.zip ./

clean:
	rm *.zip -f

