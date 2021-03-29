clc
clear all
pos=load('chair_coordinates.txt');
vec=load('chair_normal_coordinates.txt');
ans=sqrt(vec(:,1).^2+vec(:,2).^2+vec(:,3).^2);
quiver3(pos(:,1),pos(:,2),pos(:,3),vec(:,1)./ans,vec(:,2)./ans,vec(:,3)./ans);