#!/bin/bash
#echo helloo
search_in_level() {
    #echo "here"
    #echo $2
    if [ -f "$2" ];then
    file="$2"
    current_path=$(pwd)
    relative_path=${current_path#"$current_dir"}
    relative_path=${relative_path#"/"}
    if [ ! "$relative_path" ];then
        relative_path+="$file"
    else
        relative_path+="/$file"
    fi
    #echo $relative_path
    if [[ "$file" == *.* ]];then
    filext="${file##*.}"
    flag=0
    
    for i in "${arr[@]}"
    do
        if [ "$filext" = "$i" ];then
        #echo $filext
        flag=1
        fi
        #echo $i
    done
    if ((flag==0));then
        #echo $filext
        if [ ! -d "$filepath/Output/$filext" ];then
            mkdir $filepath/Output/$filext
        fi
        text_file_name="desc_${filext}"
        touch $filepath/Output/$filext/$text_file_name.txt
        if [ ! -f "$filepath/Output/$filext/$file" ];then
        cp "$file" "$filepath/Output/$filext"
        echo "$relative_path" >> $filepath/Output/$filext/$text_file_name.txt
        fi
    else 
        #echo $filext
        count=$((count+1))
        #echo $count
    fi
    else 
        if [ ! -d "$filepath/Output/others" ];then
            mkdir $filepath/Output/others
        fi
        touch $filepath/Output/others/desc_others.txt
        if [ ! -f "$filepath/Output/others/$file" ];then
        cp "$file" "$filepath/Output/others"
        echo "$relative_path" >> $filepath/Output/others/desc_others.txt
        fi
    fi
    fi
}

search_in_directory() {
    cd "$1"
    for file in *
    do
        if [ -d "$file" ];then
            ((x=$2+1))
            #search_in_level $2 "$file"
            search_in_directory "$file" $x
        elif [ -f "$file" ];then
            search_in_level $2 "$file"
        fi
    done
    cd ../
}

if(($#==2));then
    filename=$2
    path=$1
elif(($#==1));then
    filename=$1
    path=.
else 
    echo "USAGE: Looks like you are having trouble in using this script."
    echo "PLease provide a working directory(optional) and a valid input file name"
    echo "as command line arguments to run this script."
    exit
fi

current_dir=$(pwd)
#cd $path
#temppath=$(pwd)
#filepath="$(dirname "$temppath")"
#cd $current_dir

#echo $filepath
while true 
do
if [ ! -f "$filename" ];then
    echo "Please provide a valid input file"
    read filename
else 
    break
fi
done

cd $path
cd ..
filepath=$(pwd)
cd $current_dir
#echo $filepath

if [ ! -d "$filepath/Output" ];then
    mkdir $filepath/Output
fi


n=0
while IFS=$'\n\r' read -r line; do
    arr[$n]=$line
    n=$((n+1))
done < $filename
#echo ${arr[*]}

count=0
search_in_directory $path 1 

echo "Type_of_file,Number_of_files">>$filepath/output.csv
cd $filepath/Output
for file in *
    do
        if [ -d "$file" ];then
            var=$(find "$file" -type f | wc -l)
            ((var=var-1))
            echo "$file,$var">>$filepath/output.csv
            #echo $file>>output.csv
            #echo $var>>output.csv
        fi
    done
echo "ignored,$count">>$filepath/output.csv