package com.iot.smartbin.config;

import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.client.builder.AwsClientBuilder;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import org.springframework.context.annotation.Configuration;

/**
 * @author Siuyun Yip
 * @version 1.0
 * @date 2023/4/4 14:16
 */
@Configuration
public class AwsS3Config {
    private String accessKey = null;
    private String secretAccessKey = null;
    private String region = null;
    private String endpoint = null;

    public AmazonS3 getS3Client() {
        AWSCredentials credentials = new BasicAWSCredentials(accessKey, secretAccessKey);
        return AmazonS3ClientBuilder.standard().withEndpointConfiguration(new AwsClientBuilder.EndpointConfiguration(endpoint, region))
                .withCredentials(new AWSStaticCredentialsProvider(credentials))
                .build();
    }

}
