yum install -y curl
cat docker-compose.yml \
    | sed "/dhost.corp.ooma.com:5043\/lte-management-service:*/c\        image: dhost.corp.ooma.com:5043\/lte-management-service:temp" \
    | sed "/build: ./d" \
    > docker-compose-temp.yml
curl -F docker-compose=@docker-compose-temp.yml $CI_SERVER/lms/$CI_BUILD_ID 
curl -X POST $CI_SERVER/run/$CI_BUILD_ID > functional-tests.out
exit $?
