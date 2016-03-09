
# "age" ontology is 5yr-segment, e.g. "0~5,5~10"
build_age_tree() {
  step=1
  a=($(seq 1 $step 100))
  la=${#a[@]}
  for (( i=1; i < la; ++i)); do
    echo "age,${a[i-1]}~${a[i]}"
  done
}

# "gender" ontology
build_gender_tree() {
  a=(male female)
  for i in "${a[@]}"; do
    echo "gender,$i"
  done
}

# given "user,birthday", create "user,age_tag"
build_user_age() {
  :
}

main() {
  build_age_tree
  build_gender_tree
}

main
