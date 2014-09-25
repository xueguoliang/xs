

target=__all_files__.c
echo '#ifdef __cplusplus' > $target
echo 'extern "C"{' >> $target
echo '#endif' >> $target
echo 'const char* g_mem_files[]={' >> $target

allfiles=`cat $1`

for i in `cat $1`; do
    echo '"'$i'",' >> $target
done
echo '};' >> $target
echo 'int g_mem_file_count=' >> $target
echo `wc -l $1 | awk {'print $1'}` >> $target 
echo ';' >> $target

echo '#ifdef __cplusplus' >> $target
echo '}' >> $target
echo '#endif' >> $target

rm $1
