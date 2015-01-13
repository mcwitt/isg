static int bisect(double a[], double x, int l, int r)
{
    int mid;

    while (r-l > 1)
    {
        mid = (l+r)/2;
        if (a[mid] < x) l = mid;
        else r = mid;
    }

    return (x < a[l]) ? l : r;
}
