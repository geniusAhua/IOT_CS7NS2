package com.iot.smartbin.mqtt.s3;

import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.S3Object;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class MyObject {
    public String getURL() throws IOException {
        String bucketName = "bucketName";
        String key = "key";

        AmazonS3 s3Client = AmazonS3ClientBuilder.defaultClient();
        GetObjectRequest getObjectRequest = new GetObjectRequest(bucketName, key);
        S3Object s3Object = s3Client.getObject(getObjectRequest);

        InputStream objectContent = s3Object.getObjectContent();
        byte[] content = objectContent.readAllBytes();
        objectContent.close();
        s3Object.close();
        return Arrays.toString(content);
    }

}
