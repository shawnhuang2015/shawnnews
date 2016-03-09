
cleanse_user_personal_tag() {
  if [[ $# != 2 ]]; then
    >&2 echo "usage: * ifile ofile"
    return 1
  fi

  echo "cleanse $1, out $2"

  # strip chars: []", replace first `,` with `;`
  cat $1 | tr -d '[]"' | sed 's/,/;/1' > $2

  # 
}


main() {
  if [[ "$1" == "user_personal_tag" ]]; then
    shift
    cleanse_user_personal_tag "$@"
  fi
}

main "$@"
