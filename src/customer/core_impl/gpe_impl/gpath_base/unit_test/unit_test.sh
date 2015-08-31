echo test_aggregator
sh compile_test_aggregator.sh
./test_aggregator > log.txt
echo test_column_meta
sh compile_test_column_meta.sh
./test_column_meta >> log.txt
echo test_context_table_message
sh compile_test_context_table_message.sh
./test_context_table_message >> log.txt
echo test_context_table
sh compile_test_context_table.sh
./test_context_table >> log.txt
echo test_expression
sh compile_test_expression.sh
./test_expression >> log.txt
echo test_operations
sh compile_test_operations.sh
./test_operations >> log.txt
echo test_ragraph
sh compile_test_ragraph.sh
./test_ragraph >> log.txt

