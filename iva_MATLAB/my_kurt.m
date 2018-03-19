function kurt=my_kurt(y)
kurt = mean(y.^4)-3*(mean(y.^2))^2;
end