for ((i=0;i<=40000;i+=5000)) do
	python get_pages.py $i 5000 &
done
